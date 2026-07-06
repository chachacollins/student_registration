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
![NOTE]
>The student id has to be in all caps

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

### /courses
Takes a GET request and returns the courses available for the department.

Returns an array of course objects, each with the following schema:

```json
[
  {
    "course_id": "CSCI101",
    "dept_code": "CSCI",
    "course_name": "Introduction to Computer Science",
    "description": "Basic programming concepts.",
    "capacity": 100,
    "enrolled_count": 73,
    "semester": "Fall 2023",
    "day_of_week": "Monday",
    "start_time": "10:00",
    "end_time": "11:30",
    "room": "A101",
    "is_active": true
  }
]
```

On error, returns:
```json
{"error": "reason"}

### /courses/<course_id>
GET request returns information about a specific course, if it exists.

Returns a course object with the following schema:
```json
{
  "course_id": "CSCI101",
  "dept_code": "CSCI",
  "course_name": "Introduction to Computer Science",
  "description": "Basic programming concepts.",
  "capacity": 100,
  "enrolled_count": 73,
  "semester": "Fall 2023",
  "day_of_week": "Monday",
  "start_time": "10:00",
  "end_time": "11:30",
  "room": "A101",
  "is_active": true
}
```

If the course is not found, returns:
```json
{"error": "Course not found"}
```

On other errors, returns the relevant status code and a JSON object:
```json
{"error": "reason"}
```

### /courses/<course_id>/register
POST request which registers the currently logged in student to the course

###/courses/<course_id>/drop
POST request which deregisters the currently logged in student from the course

### /courses/registered/
GET request which returns all the registered courses for the currently logged in student.

Returns a list of course objects, each with the following schema:
```json
[
  {
    "course_id": "CSCI101",
    "dept_code": "CSCI",
    "course_name": "Introduction to Computer Science",
    "description": "Basic programming concepts.",
    "capacity": 100,
    "enrolled_count": 73,
    "semester": "Fall 2023",
    "day_of_week": "Monday",
    "start_time": "10:00",
    "end_time": "11:30",
    "room": "A101",
    "is_active": true,
    "registered_at": "2024-04-20T15:30:00Z",
    "status": "active"
  }
]
```

On error, returns the relevant status code and a JSON object:
```json
{"error": "reason"}
```