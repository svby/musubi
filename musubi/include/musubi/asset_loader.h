/// @file
/// @author stuhlmeier
/// @date 5 January 2020

#ifndef MUSUBI_ASSET_LOADER_H
#define MUSUBI_ASSET_LOADER_H

#include "musubi/asset_registry.h"
#include "musubi/exception.h"

#include <memory>

namespace musubi {
    template<typename Asset>
    struct asset_loader;

    template<typename Asset>
    struct asset_loader<std::unique_ptr<Asset>> {
        asset_loader<Asset> loader;

        template<typename ...LoaderArgs>
        inline auto operator()(LoaderArgs &&...args) {
            return std::make_unique<Asset>(std::move(loader(std::forward<LoaderArgs>(args)...)));
        }
    };

    template<typename Asset>
    struct asset_loader<std::shared_ptr<Asset>> {
        asset_loader<Asset> loader;

        template<typename ...LoaderArgs>
        inline auto operator()(LoaderArgs &&...args) {
            return std::make_shared<Asset>(std::move(loader(std::forward<LoaderArgs>(args)...)));
        }
    };

    template<typename Asset, typename Loader = asset_loader<Asset>, typename ...LoaderArgs>
    inline Asset load_asset(const asset_registry::mpack &pack, std::string_view name, LoaderArgs &&...args) {
        const auto buffer = pack.get_buffer(name);
        if (!buffer) {
            throw resource_read_error(
                    "Could not load asset "s + std::string(name) + "from pack, asset was not read"s);
        }

        if (const auto bytes = buffer.value(); bytes) {
            Loader loader;
            return loader(bytes->data(), bytes->size(), std::forward<LoaderArgs>(args)...);
        } else {
            throw std::invalid_argument("Could not load asset "s + std::string(name) + "from pack, buffer was null");
        }
    }
}

#endif //MUSUBI_ASSET_LOADER_H
