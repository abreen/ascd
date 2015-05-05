import sys
from os.path import basename

with open('statements.h', 'w') as f:
    for path in sys.argv[1:]:
        var_name = basename(path).split('.')[0]

        f.write('const char *stmt_' + var_name)
        f.write(' = "')

        with open(path, 'r') as sql:
            data = sql.read()
            data = data.replace('"', '\\"')
            data = data.replace('\n', '\\n')

            f.write(data)

        f.write('";\n\n')
