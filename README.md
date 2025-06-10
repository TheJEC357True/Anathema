# Anathema

## Project Overview

An open-world third-person action RPG developed in Unreal Engine 5.5. This project serves as a personal learning initiative to deepen my understanding of Unreal Engine's C++ and Blueprint frameworks, and hopefully make something fun along the way.

While Anathema is an original work with custom assets and lore, its game mechanics and system design are deeply **inspired by the complex character progression, vast world structure, and detailed magic systems often found in Japanese Light Novel/Web Novel series like "Overlord" and classic MMORPGs**.

## Features & Implemented Systems

This project focuses on demonstrating my ability to build robust and scalable gameplay systems:

*   **Tiered Magic System:**
    *   Implemented a multi-tier arcane spellcasting system, allowing for progressive power growth.
    *   Designed core C++ classes (`UMyBaseSpell`) to define common spell properties (mana cost, tier, effects).
    *   Utilized Blueprints to create numerous derived, specific spells (`BP_Spell_MagicMissile`, `BP_Spell_Fireball`), demonstrating content creation on top of a C++ foundation.
    *   Implemented mana management and casting logic.

*   **Class System:**
    *   Designed a flexible character progression system with distinct "Lineages" and "Professions".
    *   Allowed highly personalised character builds based on lineage and profession choice.

*   **Quest System:**
    *   Implemented a graph-based quest management system.
    *   Designed C++ `UQuestNode` for quest definition (prerequisites, follow-ups).
    *   Designed C++ `UObjective` to be used in polymorphism for general quest objective tracking/definition.

*   **Modular Health & Combat System:**
    *   Developed a reusable `UHealthComponent` (C++) that can be attached to any actor (player, enemy, destructible environment).
    *   Implemented functions for taking damage, healing, and tracking death state.
    *   Integrated events (BlueprintAssignable) for health changes and death, allowing flexible UI updates and gameplay reactions.

*   **Game World & Level Design:**
    *   Exploration of modular level construction within Unreal Engine.

## Technical Stack & Skills Demonstrated

*   **Unreal Engine 5.5:** Proficient use of the editor, Blueprint visual scripting, and C++ game programming.
*   **C++:** Development of foundational gameplay systems, base classes, and engine interaction.
*   **Blueprints:** Rapid prototyping, content creation, visual logic for specific gameplay instances, and extending C++ functionality.
*   **Git & Git LFS:** Professional version control using Git, including large file management with Git LFS for efficient binary asset tracking, use of filters to protect sensitive information from leaking, and rewriting Git 
*   **Object-Oriented Design / Component-Based Architecture:** Extensive use of inheritance, composition, and components for modular and scalable systems.

## Getting Started

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/TheJEC357True/Anathema.git
    cd Anathema
    ```
2.  **Ensure Git LFS is installed:**
    ```bash
    git lfs install
    ```
3.  **Pull LFS objects:**
    ```bash
    git lfs pull
    ```
4.  **Open in Unreal Engine:** Double-click the `Anathema.uproject` file.
    *   If you encounter issues or need to generate Visual Studio project files (for C++ development), right-click `Anathema.uproject` and select "Generate Visual Studio project files" (requires Visual Studio 2022 Community with game development workloads).

## Future Plans

*   Implement the full race/job class progression system.
*   Expand the quest system with dialogue and objective tracking.
*   Develop a robust combat system with melee and ranged options.
*   Integrate more advanced AI behaviors for enemies.
*   Build out additional playable levels and biomes.
*   Develop a storyline to make this more like a 'proper' game.

## Contact

Feel free to connect with me for questions or collaboration:

*   **GitHub:** https://github.com/TheJEC357True
*   **LinkedIn:** https://www.linkedin.com/in/joshua-choi-297201339/
*   **Email:** choijoshua569@gmail.com

## Legal Disclaimer & Inspiration

This project is a personal, non-commercial endeavor developed purely for learning and portfolio purposes. It is an original work. The game mechanics and themes are *inspired by* common tropes in MMORPGs and certain fantasy light novel series, but all character names, lore, assets, and specific storylines are original and do not replicate or claim ownership of any existing intellectual property.