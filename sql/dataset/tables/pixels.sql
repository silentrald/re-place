CREATE TABLE pixels (
  id          BIGSERIAL NOT NULL PRIMARY KEY,
  color       INTEGER NOT NULL,
  timestamp   TIMESTAMP NOT NULL DEFAULT NOW(),
  position_x  INTEGER NOT NULL,
  position_y  INTEGER NOT NULL,
  user_id     UUID NOT NULL,
  canvas_id   INTEGER NOT NULL,

  FOREIGN KEY (user_id) REFERENCES users (id),
  FOREIGN KEY (canvas_id) REFERENCES canvases (id)
);

\echo 'pixels table created'
