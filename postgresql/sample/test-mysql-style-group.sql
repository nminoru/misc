CREATE TABLE result (id integer, content text);

INSERT INTO result VALUES (1, 'Shibuya');
INSERT INTO result VALUES (2, 'Ebisu');
INSERT INTO result VALUES (3, 'Gotanda');
INSERT INTO result VALUES (4, 'Shibuya');
INSERT INTO result VALUES (5, 'Nakameguro');

SELECT min(id) AS id,content FROM result GROUP BY content ORDER BY id;

SELECT * FROM result WHERE id IN (SELECT min(id) FROM result GROUP BY content);
