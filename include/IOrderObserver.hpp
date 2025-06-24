#pragma once

class IOrderObserver{
    public:
        virtual void onAddOrder() = 0;
        virtual void onRemoveOrder() = 0;
        virtual void onMatchOrder() = 0;
};