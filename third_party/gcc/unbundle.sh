#-*-mode:sh;indent-tabs-mode:nil;tab-width:2;coding:utf-8-*-┐
#───vi: set net ft=sh ts=2 sts=2 fenc=utf-8 :vi─────────────┘
mkdir -p o/third_party
cp -R third_party/gcc o/third_party/gcc.$$
for f in $(find o/third_party/gcc.$$ -name \*.gz); do
  gunzip $f
  chmod +x ${f%.gz}
done
mv o/third_party/gcc.$$ o/third_party/gcc
