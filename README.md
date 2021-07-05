# OGircd

OGircd is an IRC deamon, written in C++98. It was originally made as part of the 42cursus's ft_irc project.

It was made by three poeple :
- [Me](https://github.com/salaaad2)
- [aurhil](https://github.com/paure42)
- [Ted Bajrami](https://github.com/)

# Building

``` sh
git clone https://github.com/salaaad2/OGircd
cd OGircd
make -j
```

# running

``` sh
./ogircd <port> <password>
```

recommended port is 6667, but it is not set by default

# Features

As it is being worked on, the server only implements part of the official IRC RFC, but it is in a usable state nonetheless.

The client I recommend for usage with this server is [weechat](https://weechat.org)
