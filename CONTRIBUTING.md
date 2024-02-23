# Contributing Guidelines

We'd love to accept your patches! Please read this guide first.

## Identity Disclosure

This project does not accept anonymous contributions. Justine Tunney
won't merge pull requests from strangers. In order to change the Cosmo
codebase, and have your changes be upstreamed, she has to know who you
are. You're encouraged to disclose your full name and email address to
the public too, by including them in your git commit messages; however
that's not a requirement; as we're happy to respect the wishes of
contributors who prefer to remain anonymous to the public.

## Copyright Assignment

The first time you send a pull request, you need to send an email to
Justine Tunney <jtunney@gmail.com> stating that you intend to assign her
the copyright to the changes you contribute to Cosmopolitan. It only
needs to happen once. This only applies to the code you *choose* to
contribute. The email should be sent from an email address associated
with your identity. Your email should link to your pull request.

To make things easy, here's an example of a good email you can use:

> **From**: YOUR NAME (yname@gmail.com)  
> **To**: Justine Tunney (jtunney@gmail.com)  
> **Subject**: Cosmopolitan Copyright Assignment for YOUR NAME
>
> Hi Justine,
>
> I made my first contribution to Cosmopolitan in
> https://github.com/jart/cosmopolitan/pull/XXXX could you please take a
> look? I intend to assign you the copyright to the changes I contribute
> to Cosmopolitan.
>
> Thanks!

Please note that in order to give Justine the copyright, it has to be
yours to give in the first place. If you're employed, then you should
get your employer's approval to do this beforehand. Even with big
companies like Google, this process is quick and painless. Usually we
see employers granting authorization in less than one day.

If you live in a country that doesn't recognize one's ability to assign
copyright, then you may alternatively consider disclaiming it using the
language in [Unlicense](https://unlicense.org) or
[CC-0](http://creativecommons.org/share-your-work/public-domain/cc0).

If you're checking-in third party code, then you need to have headers at
the top of each source file (but never header files) documenting its
owners and the code should go in the `third_party/` folder. Every third
party project should have a `README.cosmo` file that documents its
provenance as well as any local changes you've made.

## Copyright Policy Exceptions

### Tests

You're encoraged to claim ownership of your test code. If you add a new
file under the `test/` directory, then you should put your name in the
ISC license header at the top of the file. If you add new test cases to
an existing unit test file, then you're encouraged to append a line with
your name to the existing copyright header of that file.

### Exceptional Features

Let's say you discovered a faster better way to implement `log10()` and
you want to give it to Cosmopolitan. In cases like this, it really isn't
appropriate for Justine to own your code. What you could do instead, is
write your own new and improved `log10.c` from scratch, put your name on
the top with the ISC license, and then add a `__notice()` directive so
that your name will be embedded inside every executable that links the
`log10()` function. This will help you get your name out there. Please
note you need get approval from Justine each time you want to do this.

## Style Guide

You can use clang-format to automatically format your files:

```sh
clang-format -i -style=file tool/net/redbean.c
```

If you use Emacs this can be automated on save for Cosmopolitan using
[tool/emacs/cosmo-format.el](tool/emacs/cosmo-format.el).
