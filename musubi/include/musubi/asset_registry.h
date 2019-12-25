//
// Created by stuhlmeier on 12/23/19.
//

#ifndef MUSUBI_ASSET_REGISTRY_H
#define MUSUBI_ASSET_REGISTRY_H

#include "musubi/input.h"

#include <any>
#include <cstddef>
#include <filesystem>
#include <functional>
#include <map>
#include <memory>
#include <unordered_map>

namespace musubi {
    using std::byte;

    class asset_registry final {
    public:
        struct mpack final {
            friend class asset_registry;

            [[nodiscard]] std::optional<const std::vector<byte> *> get_buffer(std::string_view name) const;

        private:
            struct mpack_entry {
                std::vector<byte> buffer;
            };

            std::map<std::string, mpack_entry, std::less<>> contents;
        };

        LIBMUSUBI_DELCP(asset_registry)

        static std::unique_ptr<asset_registry> from_paths(std::initializer_list<std::filesystem::path> paths);

        asset_registry(asset_registry &&other) noexcept;

        asset_registry &operator=(asset_registry &&other) noexcept;

        ~asset_registry();

        std::unique_ptr<mpack> load_pack(const std::string &packName);

    private:
        asset_registry() noexcept;

        struct pack_data;

        std::unordered_map<std::string, std::unique_ptr<pack_data>> packs;
    };
}

#endif //MUSUBI_ASSET_REGISTRY_H
