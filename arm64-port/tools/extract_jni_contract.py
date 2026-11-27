#!/usr/bin/env python3
"""List ACC_NATIVE methods from an APK classes.dex without third-party packages."""
import argparse
import struct
import zipfile


def uleb(data, off):
    value = shift = 0
    while True:
        byte = data[off]
        off += 1
        value |= (byte & 0x7f) << shift
        if byte < 0x80:
            return value, off
        shift += 7


def read_dex(data):
    def u32(off):
        return struct.unpack_from("<I", data, off)[0]

    string_count, string_off = struct.unpack_from("<II", data, 0x38)
    type_count, type_off = struct.unpack_from("<II", data, 0x40)
    proto_count, proto_off = struct.unpack_from("<II", data, 0x48)
    method_count, method_off = struct.unpack_from("<II", data, 0x58)
    class_count, class_off = struct.unpack_from("<II", data, 0x60)

    strings = []
    for i in range(string_count):
        off = u32(string_off + i * 4)
        _, off = uleb(data, off)  # UTF-16 length; UTF-8 bytes are zero-terminated.
        end = data.index(b"\\0", off)
        strings.append(data[off:end].decode("utf-8", "replace"))

    types = [strings[u32(type_off + i * 4)] for i in range(type_count)]
    protos = []
    for i in range(proto_count):
        _, ret_idx, params_off = struct.unpack_from("<III", data, proto_off + i * 12)
        params = []
        if params_off:
            count = u32(params_off)
            params = [types[struct.unpack_from("<H", data, params_off + 4 + j * 2)[0]]
                      for j in range(count)]
        protos.append((types[ret_idx], params))

    methods = []
    for i in range(method_count):
        class_idx, proto_idx, name_idx = struct.unpack_from("<HHI", data, method_off + i * 8)
        methods.append((types[class_idx], strings[name_idx], protos[proto_idx]))

    found = []
    for i in range(class_count):
        class_idx, _, _, _, _, _, class_data_off, _ = struct.unpack_from(
            "<IIIIIIII", data, class_off + i * 32)
        if not class_data_off:
            continue
        off = class_data_off
        static_fields, off = uleb(data, off)
        instance_fields, off = uleb(data, off)
        direct_methods, off = uleb(data, off)
        virtual_methods, off = uleb(data, off)

        for field_count in (static_fields, instance_fields):
            field_idx = 0
            for _ in range(field_count):
                diff, off = uleb(data, off)
                field_idx += diff
                _, off = uleb(data, off)

        for method_count_in_class in (direct_methods, virtual_methods):
            method_idx = 0
            for _ in range(method_count_in_class):
                diff, off = uleb(data, off)
                method_idx += diff
                access_flags, off = uleb(data, off)
                _, off = uleb(data, off)  # code_off
                if access_flags & 0x100:
                    owner, name, (ret, args) = methods[method_idx]
                    descriptor = "(" + "".join(args) + ")" + ret
                    found.append((owner, name, descriptor, access_flags))
    return found


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("apk")
    args = parser.parse_args()
    with zipfile.ZipFile(args.apk) as apk:
        dex = apk.read("classes.dex")
    entries = read_dex(dex)
    for owner, name, descriptor, flags in sorted(entries):
        print(f"{owner}->{name}{descriptor} [flags=0x{flags:x}]")
    print(f"TOTAL_NATIVE_METHODS={len(entries)}")


if __name__ == "__main__":
    main()
