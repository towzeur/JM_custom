import os


def normalize_content(text):
    # Convertit CRLF → LF et supprime les espaces en fin de ligne
    return "\n".join(line.rstrip() for line in text.replace("\r\n", "\n").split("\n"))


def files_are_different(src_path, dst_path):
    if not os.path.exists(dst_path):
        return True
    try:
        with (
            open(src_path, "r", encoding="utf-8") as f1,
            open(dst_path, "r", encoding="utf-8") as f2,
        ):
            return normalize_content(f1.read()) != normalize_content(f2.read())
    except UnicodeDecodeError:
        # Si ce n’est pas un fichier texte, considérer qu’il est différent (ex: binaire)
        return True


def copy_with_normalization(src_dir, dst_dir):
    for root, _, files in os.walk(src_dir):
        for file in files:
            src_file = os.path.join(root, file)
            rel_path = os.path.relpath(src_file, src_dir)
            dst_file = os.path.join(dst_dir, rel_path)

            os.makedirs(os.path.dirname(dst_file), exist_ok=True)

            if files_are_different(src_file, dst_file):
                try:
                    with open(src_file, "r", encoding="utf-8") as f:
                        content = normalize_content(f.read())
                    with open(dst_file, "w", encoding="utf-8", newline="\n") as f:
                        f.write(content)
                    print(f"[MODIFIED] {rel_path}")
                except UnicodeDecodeError:
                    # Fichier binaire (ex: image, binaire C), copie brutale
                    with open(src_file, "rb") as fsrc, open(dst_file, "wb") as fdst:
                        fdst.write(fsrc.read())
                    print(f"[BINARY COPY] {rel_path}")
            else:
                print(f"[UNCHANGED] {rel_path}")


if __name__ == "__main__":
    src_dir = "F:/Desktop/vie/WORK/JM_custom/jm_coffee"
    dst_dir = "F:/Desktop/vie/WORK/JM_custom"

    copy_with_normalization(src_dir, dst_dir)
