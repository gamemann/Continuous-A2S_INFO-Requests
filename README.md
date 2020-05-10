# Continuous A2S_INFO Requests
## Description
A small C program that continuously sends A2S_INFO requests to a certain IP and port over the UDP protocol using threading. It checks to see if a response is received. If no response is received, it will print out the date. This is useful for debugging A2S_INFO packets being sent to a game server.

## Usage & Examples
Here is the usage:

```
a2s_info <IP> <Port> <Time Out>
```

Here's an example:

```
./a2s_info 92.119.148.30 27015 1
```

## Credits
* [Christian Deacon](https://www.linkedin.com/in/christian-deacon-902042186/) - Creator.