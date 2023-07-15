import argparse
import os


def generate_template(fname):
    path = f'tests/{fname}'
    try:
        os.mkdir(path)

        with open(f'{path}/{fname}.hpp', 'w+') as file:

            file.writelines(f'#ifndef {fname.upper()}_HPP\n')
            file.writelines(f'#define {fname.upper()}_HPP\n')
            file.writelines('\n')
            file.writelines('#include "bonsai/tests/test.hpp"\n')
            file.writelines('\n')
            file.writelines(f'extern void {fname}(void);\n')
            file.writelines('\n')
            file.writelines(f'#endif\n')

        with open(f'{path}/{fname}.cpp', 'w+') as file:

            file.writelines(f'#include "{fname}.hpp"\n')
            file.writelines('\n')
            file.writelines(f'void {fname}(void)' + ' { }\n')

        print(f'created template in bonsai/tests/{fname}')

    except Exception as e:
        print(e)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('test_name')
    args = parser.parse_args()
    generate_template(args.test_name)
