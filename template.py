import argparse
import os


def generate_template(fname):
    path = f'tests/{fname}'
    try:
        os.mkdir(path)

        def write_line(f, s=None): return f.writelines(
            s + '\n') if s else f.writelines('\n')

        with open(f'{path}/{fname}.hpp', 'w+') as f:

            write_line(f, f'#ifndef {fname.upper()}_HPP')
            write_line(f, f'#define {fname.upper()}_HPP')
            write_line(f)
            write_line(f, '#include "bonsai/tests/test.hpp"')
            write_line(f)
            write_line(f, f'extern void {fname}(void);')
            write_line(f)
            write_line(f, f'#endif')

        with open(f'{path}/{fname}.cpp', 'w+') as f:

            write_line(f, f'#include "{fname}.hpp"')
            write_line(f)
            write_line(f, f'void {fname}(void)' + ' { }')
        
        print(f'created template in bonsai/tests/{fname}')

    except Exception as e:
        print(e)


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('test_name')
    args = parser.parse_args()
    generate_template(args.test_name)
