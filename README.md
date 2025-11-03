# protobuf-cpp

[Protocol Buffers](https://protobuf.dev/) are an extensible binary serialization format. While Google provides a protocol buffer compiler that generates source code based on a protobuf (`.pb`) file, I believe reflection in C++26 allows us to remove the need for generated source code altogether. This project aims to implement protocol buffer serialization and deserializing compatible with Google's solution.

# Objectives
- [x] Encode/decode Varint (variable size integer)
- [x] Encode/decode Record
- [ ] Encode/decode fixed-size integers and floats
- [ ] Encode/decode arbitrary byte sequence
- [ ] Encode/decode other Records
- [ ] Verify compatibility with generated protobuf serialization
- [ ] Handle malformed inputs during deserialization
- [ ] Encode/decode trivially-copyable structs with only scalar members using manual mapping to custom types
- [ ] Encode/decode trivially-copyable types using reflection
- [ ] Encode/decode trivially-copyable nested structs using reflection
- [ ] Generate protobuf syntax `.pb` files from types using reflection
- [ ] Benchmark against code generated with `protoc`
- [ ] Allow compile-time checks for whether type A is compatible with type B
