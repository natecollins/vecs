==================================
VECS ENTITY COMPONENT SYSTEM
==================================


Entity
    - id-entity
    - list-components
ComponentBase
    - type-id-component
<DefinedComponent> : ComponentBase
    - Data defined
Component<DefinedComponent>
    - Instance of DefinedComponent
    - id-component
System
    - Code/behavior ??
Event
    - Thrown by systems
    - Always has a event type
Receiver
    - Can subscribe/receive thrown events
    - Typically implemented by a system    
Manager
    - assign-id-counters
    - list-entities
    - list-component-types
    - Holds references to all entities
    - Generates IDs for entities
    - Creation/destruction of entities
    - Manage subscription to event types


