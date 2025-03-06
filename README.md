# PostgreSQL Hook Demo Extension

This extension demonstrates how to use various PostgreSQL hooks in a C extension.

## Development Environment

This project uses VS Code with Dev Containers to provide a consistent development environment (postgres17 + build chain)

## Building the Extension

For reasons I havent' worked out yet, you might need to `sudo /entrypoint.sh` once you start. I've no idea (yet) why it's not being called. Once you've opened the project in the Dev Container, you can build the extension with:

```bash
cd pg_hook_demo
make
sudo make install
```

## Testing the Extension

After building and installing, you can connect to the PostgreSQL server and create the extension:

```bash
psql -U postgres
```

Then in the PostgreSQL prompt:

```sql
CREATE EXTENSION pghooks;
SELECT pg_hook_demo_test();
```

You should see various NOTICE messages for each hook that's triggered, including the transaction start hook.