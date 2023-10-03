![Logo|100x100](./docs/images/wirrigator_logo.jpg)

# Wirrigator

Little IoT irrigation project based on Wemos-D1 mini board ($2 board)

[![MIT License](https://img.shields.io/badge/License-MIT-green.svg)](https://choosealicense.com/licenses/mit/)
[![size](https://img.shields.io/github/repo-size/brinth/wirrigator)](https://github.com/brinth/wirrigator)
[![lang](https://img.shields.io/github/languages/top/brinth/wirrigator)](https://github.com/brinth/wirrigator)
[![commits](https://img.shields.io/github/commit-activity/t/brinth/wirrigator)](https://github.com/brinth/wirrigator)
[![open issues](https://img.shields.io/github/issues-raw/brinth/wirrigator)](https://github.com/brinth/wirrigator)
[![closed issues](https://img.shields.io/github/issues-closed/brinth/wirrigator)](https://github.com/brinth/wirrigator)
[![pull request](https://img.shields.io/github/languages/top/brinth/wirrigator)](https://github.com/brinth/wirrigator)
[![contributers](https://img.shields.io/github/contributors/brinth/wirrigator)](https://github.com/brinth/wirrigator)
[![discord](https://img.shields.io/discord/1155244241874853939)](https://discord.gg/PtwaUKvFpX)
[![forks](https://img.shields.io/github/forks/brinth/wirrigator)](https://github.com/brinth/wirrigator)
[![star](https://img.shields.io/github/stars/brinth/wirrigator)](https://github.com/brinth/wirrigator)
[![Smoke test for FW Build](https://github.com/brinth/wirrigator/actions/workflows/smoke.yml/badge.svg)](https://github.com/brinth/wirrigator/actions/workflows/smoke.yml)
[![Generate and Host Docs](https://github.com/brinth/wirrigator/actions/workflows/docs.yml/badge.svg)](https://github.com/brinth/wirrigator/actions/workflows/docs.yml)

## Build

Requirements 
- Hardware
    - [Wemos D1 mini](https://www.wemos.cc/en/latest/d1/d1_mini.html)
    - 12V Solenoid Valve
    - Hall Effect Flowmeter
    - [PCB](https://oshwlab.com/brinth/wemos-irrigation-controller_copy#P3)
- Software
    - [ESP8266-RTOS-SDK](https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/get-started/index.html)
    - bash
    - python (version 2.7)
    - doxygen
> NOTE: Consider using Dockerized ESP8266_RTOS_SDK for running your project [repo](https://github.com/brinth/ESP8266_RTOS_SDK_Docker)
```bash
 ./prebuild.sh
  make flash monitor
```

## License

[MIT](https://choosealicense.com/licenses/mit/)


## Authors

- [@brinth](https://www.github.com/brinth)


## Contributing

Contributions are always welcome!

See [contributing.md](docs/rules/contributing.md) for ways to get started.

Please adhere to this project's [CODE OF CONDUCT](docs/rules/CODE_OF_CONDUCT.md).


## Documentation

[Software](https://brinth.github.io/wirrigator/docs/software/html/annotated.html)

## Contact
- [Discord](https://discord.gg/PtwaUKvFpX)
- [e-mail](brinthkhanna@yahoo.com)
