[//]: # (SPDX-License-Identifier: CC-BY-4.0)

This patch to Valgrind allows detecting secret-dependent division
instructions by flagging variable-latency instruction depending
on uninitialized data.

It is part of the KyberSlash paper[^KyberSlash].

<!--- bibliography --->
[^KyberSlash]: Bernstein, Bhargavan, Bhasin, Chattopadhyay, Chia, Kannwischer, Kiefer, Paiva, Ravi, Tamvada: KyberSlash: Exploiting secret-dependent division timings in Kyber implementations, [https://kyberslash.cr.yp.to/papers.html](https://kyberslash.cr.yp.to/papers.html)
