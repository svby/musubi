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
    inline Asset load_asset(const asset_registry::mpack::pack_item &item, LoaderArgs &&...args) {
        Loader loader;
        return loader(item, std::forward<LoaderArgs>(args)...);
    }

    template<typename Asset, typename Loader = asset_loader<Asset>, typename ...LoaderArgs>
    inline Asset load_asset(const asset_registry::mpack &pack, std::string_view name, LoaderArgs &&...args) {
        if (const auto item = pack[name]; item) {
            return load_asset<Asset, Loader, LoaderArgs...>(*item, std::forward<LoaderArgs>(args)...);
        } else {
            throw resource_read_error(
                    "Could not load asset "s + std::string(name) + "; item was not read"s);
        }
    }
}

// Basic asset loaders
namespace musubi {
    template<typename CharT, typename Traits, typename Allocator>
    struct asset_loader<std::basic_string<CharT, Traits, Allocator>> {
        using string_type = std::basic_string<CharT, Traits, Allocator>;

        string_type operator()(const asset_registry::mpack::pack_item &item) {
            if (const auto buffer = item.get_buffer(); buffer) {
                const std::vector<std::byte> &bytes = buffer->get();
                const auto chars = reinterpret_cast<const unsigned char *>(bytes.data());
                return string_type(chars, chars + bytes.size());
            } else throw asset_load_error::no_buffer(item.get_name());
        }
    };
}

#endif //MUSUBI_ASSET_LOADER_H
