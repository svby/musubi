//
// Created by stuhlmeier on 12/23/19.
//

#ifndef MUSUBI_ASSET_REGISTRY_H
#define MUSUBI_ASSET_REGISTRY_H

#include "musubi/input.h"

#include <nlohmann/json.hpp>

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
        /// @details Asset packs can only be loaded into memory in full,
        /// as not all archive formats support random access.
        /// Loaded contents ("items") can be retrieved via @ref get_item().
        /// @see pack_item
        struct mpack final {
        public:
            friend class asset_registry;

            /// @brief A single resource, loaded as part of a resource pack.
            /// @details Resources may or may not correspond to actual loaded files.
            /// @see mpack
            struct pack_item {
            public:
                /// @brief Constructs a pack item with the specified name, optional buffer, and JSON configuration.
                /// @param name the resource name
                /// @param buffer the optional loaded contents
                /// @param config the configuration
                pack_item(std::string name, std::optional<std::vector<byte>> buffer, nlohmann::json config);

                /// @brief Retrieves this resource's name.
                /// @return this resource's name
                [[nodiscard]] const std::string &get_name() const;

                /// @brief Retrieves this resource's loaded buffer, if present.
                /// @return this resource's buffer, or nullopt
                [[nodiscard]] std::optional<std::reference_wrapper<const std::vector<byte>>> get_buffer() const;

                /// @brief Retrieves this resource's JSON configuration.
                /// @return this resource's configuration object
                [[nodiscard]] const nlohmann::json &get_configuration() const;

            private:
                std::string name;
                std::optional<std::vector<byte>> buffer;
                nlohmann::json config;
            };

            /// @details Retrieves the resource with the specified name.
            /// @param name the resource name
            /// @return the resource with the specified name
            [[nodiscard]] std::optional<std::reference_wrapper<const pack_item>>
            get_item(std::string_view name) const;

            /// @details Retrieves the resource with the specified name.
            /// @param name the resource name
            /// @return the resource with the specified name
            std::optional<std::reference_wrapper<const pack_item>>
            operator[](std::string_view name) const noexcept(noexcept(get_item(name)));

        private:
            std::map<std::string, pack_item, std::less<>> contents;
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
