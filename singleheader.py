
SEPARATOR = '''
/*
 * -----------------------------------------------------------------------------
 * ----| IMPLEMENTATION |-------------------------------------------------------
 * -----------------------------------------------------------------------------
 */
'''

def createSingleheader(name: str, dir: str, header: str, sources: list[str]) -> None:
    with open(f"./{name}.h", 'w') as out:

        # header
        with open(f"./{dir}/{header}") as f:
            out.write(f.read())

        out.write(SEPARATOR)

        # implementation guard
        define = f"{name.upper()}_IMPLEMENTATION"
        out.write(f"#ifdef {define}\n")

        # source files
        for file in sources:
            with open(f"./{dir}/{file}") as f:
                out.writelines(f.readlines()[1:])
                out.write("\n\n")

        out.write(f"#endif /* !{define} */\n\n")

        # license
        out.write("/*\n")
        with open(f"./LICENSE") as f:
            out.write(f.read())
        out.write("*/")

if __name__ == '__main__':
    header = "minimal.h"

    sources = [
        "minimal.c",
        "input.c",
        "event.c",
        "platform_windows.c"
    ]

    createSingleheader("minimal", "src", header, sources)