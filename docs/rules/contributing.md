# Contributing to wirrigator

Thank you for your interest in contributing to wirrigator! We welcome contributions from the community and appreciate your help in making this project better.

Before you start contributing, please read and follow these guidelines to ensure a smooth and productive collaboration.

## Table of Contents
- [Code of Conduct](#code-of-conduct)
- [How to Contribute](#how-to-contribute)
  - [Reporting Bugs](#reporting-bugs)
  - [Suggesting Enhancements](#suggesting-enhancements)
  - [Code Contribution](#code-contribution)
  - [Documentation](#documentation)
- [Development Setup](#development-setup)
- [Branch usage](#branch-usage)
- [License](#license)

## Code of Conduct

Please review our [Code of Conduct](CODE_OF_CONDUCT.md) before participating in this project. We aim to create a friendly and inclusive environment for all contributors.

## How to Contribute
We welcome contributions from the community! Whether you're reporting a bug, suggesting an enhancement, or contributing code, your input is valuable. Below are some guidelines to help you get started

### Reporting Bugs

If you encounter a bug or issue with the project, please consider the following steps:

1. Check the [GitHub Issues](https://github.com/brinth/wirrigator/issues) to see if the issue has already been reported.

2. If it hasn't been reported, open a new issue with a clear and detailed description of the problem, including any relevant error messages or logs.

3. Use the appropriate issue template, if provided.

### Suggesting Enhancements

We welcome suggestions for improving the project. To suggest an enhancement:

1. Check the [GitHub Issues](https://github.com/brinth/wirrigator/issues) to see if the enhancement has already been suggested.

2. If not, open a new issue with a clear and detailed description of your suggestion. Explain why it would be beneficial and provide any relevant details.

3. Use the appropriate issue template, if provided.

### Code Contribution

We encourage contributions from the community. To contribute code:

1. Fork the project on GitHub.

2. Create a new branch for your feature or bug fix.

3. Write clean, well-documented code that follows the project's coding standards.

4. Submit a pull request (PR) to the `main` branch of this repository. Ensure that your PR includes a clear description of the changes you've made.

5. Your PR will be reviewed by project maintainers, and feedback may be provided.

6. Once your PR is approved, it will be merged into the main branch, and your contribution will be acknowledged.

### Documentation

1. Documentation is integrated to the source code using Doxygen

2. When writing new class, function or while creating new file adhere to Doxygen commenting format

3. Generate the docs manually using doxygen command which populates html & latex file under docs/software folder

4. Github Workflow is already set to publish the documents to github.io

## Development Setup

If you want to set up the project locally for development, please follow the instructions in the [README](README.md) or refer to the project's documentation.

## Branch Usage
1. Use `feature/*` branch name prefix for any feature development
2. Use `release/*` branch for releasing latest stable FW, also tag the commit used to build with version number (v.#.# Major number & Minor number)
3. Use `docs/*` feature for updating docs/ folder using doxygen (NOTE: this is important as github workflows expect this branch to handle all docs folder update)
4. Use `bugfix/*` branch for fixing any bugs or issues
5. Use `hotfix/*` branch for any quick bugfix on top of release branch

## License

By contributing to this project, you agree to the [License](LICENSE) terms and conditions.

Thank you for your contributions and support!
