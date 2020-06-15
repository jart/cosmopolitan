# Cosmopolitan

Cosmopolitan builds run natively on most platforms without dependencies,
because it implements the subset of the C library, compiler runtimes and
system call interfaces that've achieved near universal consensus amongst
all major platforms, e.g. Linux/BSD/XNU/NT. Containerization is achieved
by having binaries also be ZIP files. Modern static stock GNU toolchains
are provided in a hermetic mono repo for maximum historical determinism.

Here's how you can get started by printing cat videos inside a terminal:

    make -j8 o//tool/viz/printvideo.com
    wget https://justine.storage.googleapis.com/cats.mpg
    o//tool/viz/printvideo.com cats.mpg
    unzip -vl o//tool/viz/printvideo.com

Cosmopolitan provides a native development environment similar to those
currently being offered by RedHat, Google, Apple, Microsoft, etc. We're
different in two ways: (1) we're not a platform therefore we don't have
any commercial interest in making our tooling work better on one rather
than another; and (2) we're only focused on catering towards interfaces
all platforms agree upon. Goal is software that stands the test of time
without the costs and restrictions cross-platform distribution entails.
That makes Cosmopolitan an excellent fit for writing small CLI programs
that do things like heavyweight numerical computations as a subprocess.

## Licensing

Cosmopolitan is Free Software licensed under the GPLv2. The build config
**will embed all linked sources inside your binaries** so the compliance
is easy while facilitating trust and transparency similar to JavaScript.
You can audit your source filesystem using ZIP GUIs e.g. WIN10, InfoZip.

### Commercial Support

If you want to be able to distribute binaries in binary form only, then
please send $1,000 to jtunney@gmail.com on PayPal for a license lasting
1 year. Please be sure to provide your contact information, and details
on whether or not the license is for an individual or a corporation. If
you want your license to last 5 years, send $10,000 to the author above
who is Justine Tunney <jtunney@gmail.com>. This README will be updated,
if pricing and other details should change. Reach out for more details.

## Contributing

We'd love to accept your patches! Before we can take them, we have to
jump through one legal hurdle. Please write an email to Justine Tunney
<jtunney@gmail.com> saying you agree to give her copyright ownership to
any changes you contribute to Cosmopolitan. We need to do that in order
to dual license Cosmopolitan. Otherwise we can't tax corporations that
don't want to share their code with the community.
