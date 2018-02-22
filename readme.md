libutaust
---

Parsing and writing UST (UTAU Sequence Text) files in C.

This is one of the subprojects under Synthesizer V that I had to implement in order to make use of the vast amount of community-sourced lyric-embedded music scores for the testing of the said software.

* BSD license.

* No dependency other than the standard library.

* Does not yet support the parsing of attribute `Envelope`, `VBR`, etc. These attributes are stored as strings in structure `utaust_note`.

* Note duration and position are stored in double-precision floating points. Use of `utaust_iterator` to compute the onset time of a note is recommended since it implements high-accuracy [Kahan summation algorithm](https://en.wikipedia.org/wiki/Kahan_summation_algorithm); accumulated summation/subtraction errors are on the order of O(1). However, errors will accumulate linearly if the calling program switches back and forth between `utaust_iterator_next` and `utaust_iterator_prev`.

* You may need to tweak compiler optimization settings if you see an error greater than 1e-15 sec when running the test. I highly doubt if the accumulated error would ever be an issue though, and writing this explanation is taking probably longer than what it took to implement the sum algorithm.
