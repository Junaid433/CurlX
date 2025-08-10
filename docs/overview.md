# CurlX Documentation Overview

CurlX is a modern C++ HTTP client library that wraps libcurl with a clean, object-oriented API. This documentation provides details on its architecture, API, and advanced usage.

## Contents
- [Getting Started](getting_started.md)
- [API Reference](api_reference.md)
- [Advanced Usage](advanced.md)
- [Contributing](CONTRIBUTING.md)

---

## Design Principles
- **Simplicity:** Easy-to-use, chainable API for common HTTP tasks.
- **Extensibility:** Modular design for advanced use cases.
- **Performance:** Built on top of libcurl for robust, efficient networking.

## Main Components
- **Session:** Manages connection, cookies, and default settings. Provides methods for each HTTP verb.
- **REQUEST:** Configures request details (URL, method, headers, body, etc.) with chainable setters.
- **RESPONSE:** Contains status code, headers, body, cookies, and utility methods (`ok()`, `text()`, `json()`).

For more details, see the individual documentation files in this folder.