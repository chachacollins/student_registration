CREATE TABLE students (
    student_id    VARCHAR(50)   PRIMARY KEY,
    first_name    VARCHAR(100),
    last_name     VARCHAR(100),
    email         VARCHAR(100)  UNIQUE,
    password_hash VARCHAR(255),
    year_of_study INT,
    is_active     BOOLEAN       DEFAULT TRUE,
    created_at    TIMESTAMP     DEFAULT CURRENT_TIMESTAMP
);
