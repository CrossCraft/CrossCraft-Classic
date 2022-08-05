/**
 * @file Player.hpp
 * @author Nathan Bourgeois (iridescentrosesfall@gmail.com)
 * @brief Player controller
 * @version 0.1
 * @date 2022-01-12
 *
 * @copyright Copyright (c) 2022
 *
 */
#pragma once
#include "../MP/Client.hpp"
#include "../UI/TextHelper.hpp"
#include "../UI/UserInterface.hpp"
#include "../World/World.hpp"
#include "AABB.hpp"
#include "Chat.hpp"
#include "Graphics/2D/FontRenderer.hpp"
#include <Graphics/2D/Sprite.hpp>
#include <Rendering/Camera.hpp>
#include <Rendering/Primitive/Rectangle.hpp>
#include <any>
#include <glm.hpp>
#include "BlockRep.hpp"
using namespace Stardust_Celeste;

namespace CrossCraft
{

    class World;
    class BlockRep;

    /**
 * @brief Player controller object
 *
 */
    class Player
    {
    public:
        /**
     * @brief Construct a new Player object
     *
     */
        Player();

        /**
     * @brief Destroy the Player object
     *
     */
        ~Player() = default;

        /**
     * @brief Updates the player position and camera object
     *
     * @param dt Delta Time
     */
        auto update(float dt, World *wrld) -> void;

        /**
     * @brief Draw the player UI
     *
     */
        auto draw(World *wrld) -> void;

        auto spawn(World *wrld) -> void;

        /**
     * @brief Get the position
     *
     * @return glm::vec3
     */
        inline auto get_pos() -> glm::vec3 { return pos; }

        /**
     * @brief Get the rotation
     *
     * @return glm::vec3
     */
        inline auto get_rot() -> glm::vec2 { return rot; }

        static auto move_forward(std::any p) -> void;
        static auto move_backward(std::any p) -> void;
        static auto move_left(std::any p) -> void;
        static auto move_right(std::any p) -> void;

        static auto move_up(std::any p) -> void;
        static auto move_down(std::any p) -> void;
        static auto move_reset(std::any p) -> void;

        static auto press_up(std::any p) -> void;
        static auto press_down(std::any p) -> void;
        static auto press_left(std::any p) -> void;
        static auto press_right(std::any p) -> void;

        static auto respawn(std::any p) -> void;

        static auto change_selector(std::any p) -> void;
        static auto inc_selector(std::any p) -> void;
        static auto dec_selector(std::any p) -> void;

        static auto toggle_inv(std::any p) -> void;
        static auto enter_chat(std::any p) -> void;
        static auto submit_chat(std::any p) -> void;
        static auto delete_chat(std::any p) -> void;
        auto psp_chat() -> void;

        int32_t selectorIDX;
        uint8_t itemSelections[9];

        uint32_t terrain_atlas;
        bool in_inventory;
        bool in_chat;

        uint8_t inventorySelection[45];

        int in_cursor_x;
        int in_cursor_y;

        glm::vec3 pos;
        glm::vec2 rot;
        ScopePtr<Chat> chat;
        MP::Client *client_ref;

        Rendering::Camera cam;

        glm::mat4 projmat, viewmat;

    private:
        auto rotate(float dt, float sense) -> void;
        auto test_collide(glm::vec3 pos, World *wrld, float dt) -> void;

        glm::vec3 vel;

        float jump_icd;
        float view_bob, view_timer;
        float cube_bob;

        ScopePtr<Graphics::G2D::Sprite> item_box;
        ScopePtr<Graphics::G2D::Sprite> selector;
        ScopePtr<Graphics::G2D::Sprite> crosshair;
        ScopePtr<Graphics::G2D::Sprite> water;
        ScopePtr<Graphics::G2D::Sprite> overlay;
        uint32_t gui_texture, water_texture, overlay_texture, font_texture;

        ScopePtr<BlockRep> blockRep;

        bool hasDir;
        AABB model;

        bool is_falling, is_underwater, is_head_water, water_cutoff;
        bool on_ground, jumping;

        ScopePtr<Rendering::Primitive::Rectangle> background_rectangle;
        ScopePtr<UserInterface> playerHUD;

        float fps_timer;
        int fps_count;
        int fps_display;
    };

    struct SelData
    {
        Player *player;
        int selIDX;
    };

    struct RespawnRequest
    {
        Player *player;
        World *wrld;
    };

} // namespace CrossCraft
