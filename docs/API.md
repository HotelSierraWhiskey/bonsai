## High Level API

```c++
void Bonsai::create_file(std::string path);
```

```c++
void Bonsai::delete_file(std::string path);
```

```c++
void Bonsai::move_file(std::string dest, std::string src);
```

```c++
void Bonsai::erase(void);
```

```c++
void Bonsai::edit_file_handle(std::string path, const std::string handle)
```

```c++
void Bonsai::edit_file_data(std::string path, const std::string data)
```

```c++
void Bonsai::edit_file_parent_addr(std::string path, const uint32_t parent_addr)
```

```c++
void Bonsai::add_child_addr(std::string path, uint32_t child_addr)
```

```c++
void Bonsai::remove_child_addr(std::string path, const uint32_t child_addr)
```
