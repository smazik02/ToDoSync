# ACTIONS

## Login
#### Request 
- Header: AUTH|LOGIN
- Body: {"username": String}
#### OK Response
- Header: OK
- Body: {_empty_}

## Task Lists
### Get All User
#### Request
- Header: TL|GET_ALL
- Body: {_empty_}
#### OK Response
- Header: OK
- Body: {"lists": List<String>}
### Create
#### Request
- Header: TL|CREATE
- Body: {"name": String}
#### OK Response
- Header: OK
- Body: {"source": "TL", "message": String}
### Join
#### Request
- Header: TL|JOIN
- Body: {"name": String}
#### OK Response
- Header: OK
- Body: {"source": "TL", "message": String}

## Tasks
### Get All
#### Request
- Header: T|GET_ALL
- Body: {"task_list_name": String}
#### OK Response
- Header: OK
- Body: {"lists": List<Task>} (Task(id: Int, title: String, description: String))
### Create
#### Request
- Header: T|CREATE
- Body: {"task_list_name": String, "task_name": String, "task_description": String}
#### OK Response
- Header: OK
- Body: {"source": "T", "message": String}
### Remove
#### Request
- Header: T|DELETE
- Body: {"task_id": Int, "task_list_name": String}
#### OK Response
- Header: OK
- Body: {"source": "T", "message": String}

## Errors
- Header: FAIL
- Body: {"source": "AUTH"/"T"/"TL", "message": String}

## Notifications
- Header: NOTIFY
- Body: {"name": String(task_list_name), "description": String}