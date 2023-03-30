if __name__ == '__main__':
    file_size = 4*1024*1024 + 1
    ones = (0, 10000, file_size - 1)
    to_write = bytearray(
        [1 if i in ones else 0 for i in range(file_size)]
        )
    with open('build/A', mode='wb') as file:
        file.write(to_write)
