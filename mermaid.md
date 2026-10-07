```mermaid
graph TD
    A[Start] --> B[fill out students'mark]
    B --> C{mark >=60?}
    C --Yes -->D[Passing grade]
    C -- No -->E[Fail]
    D -->F[End]
    E -->F