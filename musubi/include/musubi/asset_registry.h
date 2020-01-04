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
#include <optional>
#include <unordered_map>

namespace musubi {
    using std::byte;

    /// @brief An asset loader.
    class asset_registry final {
    public:
        /// @brief An individual asset pack, loaded by an @ref asset_registry.
        /// @details Asset packs can currently only be loaded into memory in full,
        /// as not all archive formats support random access.
        /// Resource byte buffers can be retrieved via @ref get_buffer().
        struct mpack final {
            friend class asset_registry;

            /// @brief Retrieves a loaded resource's byte buffer.
            /// @details If the resource does not exist or no buffer was loaded, `nullopt` is returned.
            /// @param name the resource name
            /// @return the loaded resource, or nullopt
            [[nodiscard]] std::optional<const std::vector<byte> *> get_buffer(std::string_view name) const;

        private:
            struct mpack_entry {
                std::vector<byte> buffer;
            };

            std::map<std::string, mpack_entry, std::less<>> contents;
        };

        LIBMUSUBI_DELCP(asset_registry)

        /// @brief Constructs an asset registry, searching the specified search paths for asset packs.
        /// @details If any discovered pack does not explicitly specify a pack name in its pack.json file,
        /// the pack filename is used instead.
        /// @param paths the paths to recursively search for asset packs
        /// @return the newly-constructed asset registry
        static std::unique_ptr<asset_registry> from_paths(std::initializer_list<std::filesystem::path> paths);

        /// @details Move constructor; `other` becomes invalid.
        /// @param[in,out] other the registry to move from
        asset_registry(asset_registry &&other) noexcept;

        /// @details Move assignment operator; `other` becomes invalid.
        /// @param[in,out] other the registry to move from
        /// @return this
        asset_registry &operator=(asset_registry &&other) noexcept;

        /// @brief Destroys this asset registry.
        ~asset_registry();

        /// @brief Loads the specified asset pack into memory.
        /// @param packName the asset pack name, as loaded by @ref asset_registry::from_paths()
        std::unique_ptr<mpack> load_pack(const std::string &packName);

    private:
        asset_registry() noexcept;

        struct pack_data;

        std::unordered_map<std::string, std::unique_ptr<pack_data>> packs;
    };
}

#endif //MUSUBI_ASSET_REGISTRY_H
