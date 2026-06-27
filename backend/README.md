# Students Registration Backend

This is the backend of the students registration program.

# Build
To build this backend you will need:
1. A c++ compiler with support for at least c++20.
2. libsodium
3. crow
4. nlohmann json
5. libpqxx

You can then run:

```bash
make build
```

# Serve
```bash
make serve
```

# EndPoints
### /register
takes the following json object
```json
{
  "student_id": "STU12345",
  "first_name": "John",
  "last_name": "Doe",
  "email": "john.doe@university.edu",
  "password": "urmom",
  "year_of_study": 3
}
```

on sucess it returns status code 201 and json object:
```json
{"success": true}
```

on error it returns the relevant status code and json object:
```json
{"error": "reason"}
```

### /login
takes as input:
```json
{
  "email": "john.doe@university.edu",
  "password": "urmom"
}
```
on success it returns:

```json
{
  "success": true,
  "token": "jwt-token to be used for authorization"
}
```

on error it returns the relevant status code and json object:
```json
{"error": "reason"}
```
