flowchart LR
    Input["Input Command"] --> Parse["Parse Command"]
    Parse --> IsCD{"Is CD?"}
    IsCD -->|Yes| ChangeDir["Change Directory"]
    IsCD -->|No| Other["Execute Other"]
    ChangeDir --> Success{"Success?"}
    Success -->|Yes| Print["Print New Path"]
    Success -->|No| Error["Print Error"]
    Print --> Next["Continue"]
    Error --> Next
    Other --> Next

    style Input fill:#f9f,stroke:#333,color:#000
    style Print fill:#9f9,stroke:#333,color:#000
    style Error fill:#ff9,stroke:#333,color:#000