-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pghooks" to load this file. \quit

-- Create a test function to verify the extension is loaded
CREATE OR REPLACE FUNCTION pghooks_demo_test()
RETURNS text
AS 'MODULE_PATHNAME', 'pg_hook_demo_test'
LANGUAGE C STRICT;

-- Comment to help users
COMMENT ON EXTENSION pghooks IS 'PostgreSQL extension demonstrating various hooks';