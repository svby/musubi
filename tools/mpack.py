#!/usr/bin/env python3
"""
Generates asset packs from a specified list of directories (or pack.json files.)
"""
import argparse
import io
import json
import os
import tarfile
from pathlib import Path
from typing import Optional


class MPack:
    @staticmethod
    def log(message: str) -> None:
        print(f"mpack: {message}")

    def info(self, message: str) -> None:
        MPack.log(f"info:  {message}")

    def error(self, message: str) -> None:
        MPack.log(f"error: {message}")

    def verbose(self, message: str) -> None:
        if self.is_verbose:
            MPack.log(f"debug: {message}")

    def __init__(self, args: argparse.Namespace) -> None:
        self.is_verbose = args.verbose
        self.output = args.output
        self.recursive = args.recursive
        self.files = args.files

        self.single = len(self.files) == 1

    def pack_from_meta(self, name: Optional[str], meta_path: Path, destination_parent: Path) -> int:
        self.info(f"packing {meta_path}")

        parent_path = meta_path.parent

        try:
            with meta_path.open("rt") as meta_file:
                meta = json.loads(meta_file.read())
        except FileNotFoundError:
            self.error(f"{meta_path}: metadata file does not exist")
            return -1

        name = name or meta.get("name", meta_path.parent.name)

        if destination_parent.is_dir():
            destination_path = destination_parent / f"{name}.mpack"
        elif self.single:
            if destination_parent.suffix != ".mpack":
                destination_path = destination_parent.parent / f"{destination_parent.name}.mpack"
            else:
                destination_path = destination_parent
        else:
            self.error(f"{destination_parent}: destination is not a directory")
            return -1

        try:
            tar = tarfile.open(destination_path, "w:xz", dereference=True)
        except FileNotFoundError:
            self.error(f"{destination_parent}: destination does not exist")
            return -1

        # Add meta file
        meta_bytes = json.dumps(meta, separators=(",", ":")).encode("utf-8")
        meta_io = io.BytesIO(meta_bytes)

        meta_info = tarfile.TarInfo("pack.json")
        meta_info.size = len(meta_bytes)
        tar.addfile(meta_info, fileobj=meta_io)

        # Add contents
        for content in meta.get("contents", []):
            content_path = (parent_path / content).resolve()
            if parent_path not in content_path.parents:
                self.error(f"{content_path}: cannot include external asset, skipping")
                continue

            relative_content_path = content_path.relative_to(parent_path)

            try:
                tar.add(content_path, arcname=str(relative_content_path))
            except FileNotFoundError:
                self.error(f"{content_path}: asset does not exist")

        tar.close()
        return 0

    def pack(self) -> int:
        for file in self.files:
            path = Path(file).resolve()
            self.info(f"processing {path}")

            if path.is_dir():
                self.verbose(f"{path}: is a directory")
                if not self.recursive:
                    status = self.pack_from_meta(
                        path.name,
                        path / "pack.json",
                        Path(self.output or os.getcwd())
                    )
                    if status != 0:
                        return status
                else:
                    for root, _, files in os.walk(path, topdown=False):
                        if "pack.json" in files:
                            path = Path(root) / "pack.json"
                            self.verbose(f"{path}: is a file")
                            status = self.pack_from_meta(
                                None,
                                path,
                                Path(self.output or os.getcwd())
                            )
                            if status != 0:
                                return status
            elif path.exists():
                self.verbose(f"{path}: is a file")
                status = self.pack_from_meta(
                    None,
                    path,
                    Path(self.output or os.getcwd())
                )
                if status != 0:
                    return status
            else:
                self.error(f"{path}: does not exist")
                return -1


def main() -> None:
    parser = argparse.ArgumentParser(description="Generate compressed asset packs.")
    parser.add_argument(
        "--verbose", "-v", action="store_true",
        help="log more detailed information"
    )
    parser.add_argument(
        "--output", "-o", action="store",
        help="the output filename or parent directory if a single file is specified, "
             "or the output directory for all packs"
    )
    parser.add_argument(
        "--recursive", "--recur", "-r", action="store_true",
        help="recursively search all specified directories for pack.json files"
    )
    parser.add_argument(
        "files", type=str, nargs="+",
        help="a list of directories or pack.json files"
    )

    status = MPack(parser.parse_args()).pack()
    MPack.log("done")

    exit(status)


if __name__ == "__main__":
    main()
