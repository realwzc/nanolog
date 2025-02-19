# nanolog

This project is based on [Nanolog](https://github.com/zwzw1/NanoLog). This makes the output log in an observable form that can be used in practice. This project adds log segmentation and the ability to dynamically modify log levels. The average single-thread latency is about 12 ns. Results may vary depending on the cpu used.

#### Useage

Run the `sampleApplication` application in the sample folder by executing `cd sample`. The configuration file can be modified via `nanolog.conf`.

#### Todolist
The thread id that outputs the log is the nano internal id rather than the real id.