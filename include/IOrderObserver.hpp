#pragma once

class IOrderObserver{
    public:
        virtual void onAddOrder();
        virtual void onRemoveOrder();
        virtual void onMatchOrder();
};