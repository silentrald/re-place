CREATE TABLE users (
  id        UUID NOT NULL PRIMARY KEY DEFAULT uuid_generate_v4(),
  username  VARCHAR(50) NOT NULL UNIQUE,
  password  VARCHAR(100) NOT NULL -- Check which hash function to use
);

\echo 'users table created'
