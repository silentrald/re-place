CREATE TABLE canvases (
  id            SERIAL NOT NULL PRIMARY KEY,
  name          VARCHAR(100) NOT NULL,
  created_date  DATE NOT NULL DEFAULT NOW(),
  start_date    DATE,
  end_date      DATE
);

\echo 'canvases table created'
