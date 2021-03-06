#!/usr/bin/env python
import glob
import os.path
import shlex
import sys
import subprocess
import re

def usage():
    print("Usage: split_contigs_b36.py /path/to/hapmap /path/to/contigs_file")
    sys.exit(1)

def parse_contigs(file):
    contigs = {}
    with open(file, 'r') as f:
        for line in f:
            chrom, contig, start, end = line.strip('\n').split('\t')
            contigs.setdefault(chrom, {})[contig] = (start, end)
    return contigs

def main():
    argv = sys.argv
    argc = len(argv)
    if argc <= 2:
        usage()

    hapmapdir = argv[1]
    contigs_file = argv[2]
    contigs = parse_contigs(contigs_file)

    ptn = re.compile("chr([0-9]+)")
    gawk = "gawk -F' ' '{{if (NR == 1 || ({0} <= $4 && $4 <= {1})){{print $0}}}}' {2}"

    for file in glob.glob(hapmapdir + "/*"):
        filename = os.path.basename(file)
        chrom = ptn.findall(filename)
        if (len(chrom) <= 0 or os.path.isdir(file)):
            continue
        print(filename)
        chrom = chrom[0]
        for k, v in contigs[chrom].items():
            output = ptn.sub("chr"+k, filename)
            with open(output, "w") as f:
                proc = subprocess.Popen(shlex.split(gawk.format(v[0], v[1], file)), env = os.environ, stdout = f, stderr = subprocess.PIPE)
            stdout, stderr = proc.communicate()
            ret = proc.returncode

if __name__ == '__main__':
    main()

