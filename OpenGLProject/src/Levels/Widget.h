﻿#pragma once

class Widget
{
    public:
        virtual ~Widget() = default;
        virtual void OnImGuiRender() = 0;
};