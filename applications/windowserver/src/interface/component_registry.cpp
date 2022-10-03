/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                           *
 *  Ghost, a micro-kernel based operating system for the x86 architecture    *
 *  Copyright (C) 2015, Max Schlüssel <lokoxe@gmail.com>                     *
 *                                                                           *
 *  This program is free software: you can redistribute it and/or modify     *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation, either version 3 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  This program is distributed in the hope that it will be useful,          *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.    *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include <algorithm>
#include <map>

#include "interface/component_registry.hpp"

static std::map<g_ui_component_id, component_t*> components;
static g_atom components_lock = g_atomic_initialize();
static std::map<g_pid, std::map<g_ui_component_id, component_t*>> components_by_process;
static g_ui_component_id next_id = 1;

g_ui_component_id component_registry_t::add(g_pid process, component_t* component)
{
	g_atomic_lock(components_lock);
	g_ui_component_id id = next_id++;
	components[id] = component;
	components_by_process[process][id] = component;
	component->id = id;
	g_atomic_unlock(components_lock);
	return id;
}

component_t* component_registry_t::get(g_ui_component_id id)
{
	g_atomic_unlock(components_lock);
	if(components.count(id) > 0)
	{
		component_t* comp = components[id];
		g_atomic_unlock(components_lock);
		return comp;
	}
	g_atomic_unlock(components_lock);
	return 0;
}

void component_registry_t::remove_component(g_pid pid, g_ui_component_id id)
{
	g_atomic_lock(components_lock);
	if(components.count(id) > 0)
	{
		if(components_by_process.count(pid) > 0)
		{
			components_by_process[pid].erase(components_by_process[pid].find(id));
		}
		components.erase(components.find(id));
	}
	g_atomic_unlock(components_lock);
}

void component_registry_t::cleanup_process(g_pid pid)
{
	// Get components mapped for process
	g_atomic_lock(components_lock);
	auto components = &components_by_process[pid];
	g_atomic_unlock(components_lock);

	if(components)
	{
		// Put them into a list
		auto componentList = std::list<component_t*>();

		for(auto& entry : *components)
		{
			component_t* component = entry.second;
			if(component && std::find(componentList.begin(), componentList.end(), component) == componentList.end())
			{
				componentList.push_back(component);
			}
		}

		// Process items of the list
		std::list<component_t*> removedComponents;

		while(!componentList.empty())
		{
			auto component = componentList.back();
			componentList.pop_back();

			if(component->isWindow())
			{
				remove_process_components(pid, component, removedComponents);

				for(auto removed : removedComponents)
				{
					componentList.remove(removed);
				}
			}
		}

		// Remove map from registry
		g_atomic_lock(components_lock);
		components_by_process.erase(components_by_process.find(pid));
		g_atomic_unlock(components_lock);
	}
}

void component_registry_t::remove_process_components(g_pid process, component_t* component, std::list<component_t*>& removedComponents)
{
	// Never remove twice
	if(std::find(removedComponents.begin(), removedComponents.end(), component) != removedComponents.end())
	{
		return;
	}

	// Hide it
	component->setVisible(false);

	// Remove this component
	removedComponents.push_back(component);

	// Recursively remove all child elements first
	for(auto childRef : component->getChildren())
	{
		remove_process_components(process, childRef.component, removedComponents);
	}

	// Remove from registry
	remove_component(process, component->id);

	// Remove from parent
	auto parent = component->getParent();
	bool canBeDeleted = true;
	if(parent)
	{

		// Only allow to really "delete" children that are default referenced
		component_child_reference_t childReference;
		if(parent->getChildReference(component, childReference))
		{
			canBeDeleted = (childReference.type == COMPONENT_CHILD_REFERENCE_TYPE_DEFAULT);
		}

		// Remove from parent
		parent->removeChild(component);
	}

	// Finally, delete it
	if(canBeDeleted)
	{
		delete component;
	}
}
