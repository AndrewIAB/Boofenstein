#include "EditorMap.hpp"

#include <iostream>

#define IMPL_PUSH_COMMAND(TYPE) \
void EditorMap::push_command(const TYPE& command) {\
	remove_after_iterator();\
	command_log.push_back(dynamic_cast<EditorCommandBase*>(new TYPE(command))); \
	command_iterator = std::prev(command_log.end()); \
	(*command_iterator)->push(this->map_builder); \
	changed = true;\
	std::cout << "PUSH : " << const_cast<TYPE&>(command).get_command() << '\n';\
}

void EditorMap::remove_after_iterator() {
	if (command_log.size() > 0) {
		while (*command_iterator != command_log.back()) {
			delete command_log.back();
			command_log.pop_back();
		}
	}
}

IMPL_PUSH_COMMAND(EditorCommand::EmptyCommand)
IMPL_PUSH_COMMAND(EditorCommand::CreatePointCommand)
IMPL_PUSH_COMMAND(EditorCommand::CreateSegmentCommand)
IMPL_PUSH_COMMAND(EditorCommand::DeletePointCommand)
IMPL_PUSH_COMMAND(EditorCommand::DeleteSegmentCommand)
IMPL_PUSH_COMMAND(EditorCommand::MovePointCommand)
IMPL_PUSH_COMMAND(EditorCommand::ChangeMaterialCommand)

void EditorMap::roll_back_command() {
	if (command_iterator == command_log.begin()) { return; }

	std::cout << "UNDO : " << (*command_iterator)->get_command() << '\n';

	(*command_iterator)->pull(this->map_builder);
	command_iterator--;
}

void EditorMap::roll_forward_command() {
	if (command_iterator == std::prev(command_log.end())) { return; }

	command_iterator++;

	std::cout << "REDO : " << (*command_iterator)->get_command() << '\n';

	(*command_iterator)->push(this->map_builder);
}