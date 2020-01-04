//
// Created by stuhlmeier on 12/23/19.
//

#include <musubi/asset_registry.h>

#include <musubi/common.h>
#include <musubi/exception.h>

#include <archive.h>
#include <archive_entry.h>
#include <nlohmann/json.hpp>

#include <algorithm>
#include <sstream>
#include <string>
#include <unordered_set>
#include <map>
#include <utility>
#include <vector>

namespace {
    using namespace std::literals;
    using namespace std::filesystem;
    using namespace musubi;
    using namespace musubi::detail;
    using std::byte;
    using std::nullopt;
    using nlohmann::json;

    struct archive_wrapper {
        archive *wrapped;

        explicit archive_wrapper(const char *filename) {
            wrapped = archive_read_new();
            archive_read_support_filter_xz(wrapped);
            archive_read_support_format_tar(wrapped);

            int result = archive_read_open_filename(wrapped, filename, 10240);
            if (result != ARCHIVE_OK) {
                throw archive_read_error("Failed to open archive "s + filename + " (" + std::to_string(result) + ")\n");
            }
        }

        ~archive_wrapper() {
            int result = archive_read_free(wrapped);
            wrapped = nullptr;
            if (result != ARCHIVE_OK) {
                log_e("archive_wrapper") << "Failed to free archive (" << result << ")\n";
            }
        }

        operator archive *() { return wrapped; } // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)

        void read(const std::function<bool(archive_entry *)> &entryCallback) {
            archive_entry *entry{nullptr};
            while (true) {
                int status = archive_read_next_header(wrapped, &entry);
                if (status != ARCHIVE_OK) break;
                if (!entryCallback(entry)) break;
            }
        }
    };

    std::optional<std::pair<std::string, json>> process_single(const path &packPath) {
        archive_wrapper archive(packPath.c_str());

        std::optional<std::pair<std::string, json>> result = nullopt;
        archive.read([&](const auto entry) -> bool {
            if ("pack.json"s == archive_entry_pathname(entry)) {
                std::int64_t size = archive_entry_size(entry);
                std::vector<byte> buffer;
                buffer.resize(size);

                archive_read_data(archive, buffer.data(), size);
                const auto parsed = json::parse(buffer);

                const auto nameIt = parsed.find("name");
                if (nameIt == parsed.end()) result = std::make_pair(packPath.filename(), parsed);
                else result = std::make_pair(nameIt->get<std::string>(), parsed);

                return false;
            } else {
                archive_read_data_skip(archive);
                return true;
            }
        });

        return result;
    }
}

namespace musubi {
    using namespace std::literals;
    using namespace std::filesystem;
    using nlohmann::json;

    std::optional<const std::vector<byte> *> asset_registry::mpack::get_buffer(std::string_view name) const {
        const auto it = contents.find(name);
        if (it == contents.end()) return nullopt;
        return &it->second.buffer;
    }

    struct asset_registry::pack_data {
        path packPath;
        json packMeta;

        pack_data(path packPath, json packMeta) : packPath(std::move(packPath)), packMeta(std::move(packMeta)) {}
    };

    asset_registry::asset_registry() noexcept = default;

    std::unique_ptr<asset_registry> asset_registry::from_paths(std::initializer_list<path> paths = {"."}) {
        auto registry = std::unique_ptr<asset_registry>(new asset_registry());
        for (const auto &packPath : paths) {
            if (is_directory(packPath)) {
                for (const auto &child : directory_iterator(packPath)) {
                    const auto &childPath = child.path();
                    if (childPath.extension() == ".mpack") {
                        const auto childPathString = childPath.string();
                        // This could be a pack, try to load it
                        auto packInfo = process_single(childPath);
                        if (packInfo) {
                            registry->packs.emplace(
                                    packInfo->first, std::make_unique<pack_data>(childPath, std::move(packInfo->second))
                            );
                            log_i("asset_registry") << "Registered mpack " << packInfo.value().first
                                                    << " (" << childPath << ")\n";
                        } else {
                            log_e("asset_registry") << "Could not register mpack " << childPath << '\n';
                        }
                    }
                }
                log_i("asset_registry") << "Processed asset load path " << packPath << '\n';
            } else if (is_regular_file(packPath)) {
                auto packInfo = process_single(packPath);
                if (packInfo) {
                    registry->packs.emplace(
                            packInfo->first, std::make_unique<pack_data>(packPath, std::move(packInfo->second))
                    );
                    log_i("asset_registry") << "Registered mpack " << packInfo.value().first
                                            << " (" << packPath << ")\n";
                } else {
                    log_e("asset_registry") << "Could not register mpack " << packPath << '\n';
                }
            } else {
                log_e("asset_registry") << "Could not resolve asset load path " << packPath << '\n';
            }
        }

        return registry;
    }

    asset_registry::asset_registry(asset_registry &&other) noexcept
            : packs(std::move(other.packs)) {}

    asset_registry &asset_registry::operator=(asset_registry &&other) noexcept {
        packs = std::move(other.packs);
        return *this;
    }

    std::unique_ptr<asset_registry::mpack> asset_registry::load_pack(const std::string &packName) {
        const auto packIt = packs.find(packName);
        if (packIt == packs.end()) {
            throw resource_read_error("Could not load mpack "s + packName +
                                      ": no pack with that name was registered");
        }

        const auto &data = packIt->second;
        auto pack = std::make_unique<asset_registry::mpack>();

        const auto contentsIt = data->packMeta.find("contents");
        if (contentsIt == data->packMeta.end()) {
            log_w("asset_registry") << "mpack " << packName << " has no contents array; loading an empty pack\n";
            return pack;
        }

        // Map real normalized paths to filenames specified in pack.json
        std::map<std::filesystem::path, std::string> toLoad;
        for (auto it = contentsIt->begin(); it != contentsIt->end(); ++it) {
            const auto asset = it.value();
            if (asset.is_string()) {
                // Load file
                const auto assetString = asset.get<std::string>();
                toLoad.emplace(path(assetString).lexically_normal(), assetString);
            } else if (asset.is_object()) {
                // TODO
                log_e("asset_registry") << "loading complex (i.e. non-file) assets is not yet supported\n";
            }
        }

        // Read contents into buffers
        archive_wrapper archive(data->packPath.c_str());
        archive.read([&](const auto entry) -> bool {
            const auto pathname = path(archive_entry_pathname(entry)).lexically_normal();
            const auto toLoadIt = toLoad.find(pathname);
            if (toLoadIt != toLoad.end()) {
                std::int64_t size = archive_entry_size(entry);
                std::vector<byte> buffer;
                buffer.resize(size);

                archive_read_data(archive, buffer.data(), size);

                pack->contents.emplace(toLoadIt->second, mpack::mpack_entry{std::move(buffer)});
                toLoad.erase(toLoadIt);
            } else {
                archive_read_data_skip(archive);
            }
            return true;
        });

        if (!toLoad.empty()) {
            std::ostringstream error;
            error << "Could not load all required files in mpack, missing ";
            bool sep = false;
            for (const auto &item : toLoad) {
                if (sep) error << ", ";
                sep = true;
                error << item.first;
            }
            throw resource_read_error(error.str());
        }

        return pack;
    }

    asset_registry::~asset_registry() = default;
}
