#pragma once

#include <list>
#include <exception>
#include <format>
#include <optional>
#include <chrono>

#include "OrderType.h"
#include "Side.h"
#include "Usings.h"
#include "Constants.h"


class Order
{
public:
    Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity)
        : orderType_{ orderType }
        , orderId_{ orderId }
        , side_{ side }
        , price_{ price }
        , initialQuantity_{ quantity }
        , remainingQuantity_{ quantity }
        , expirationTime_{ std::nullopt }
    { }

    Order(OrderType orderType, OrderId orderId, Side side, Price price, Quantity quantity, std::chrono::system_clock::time_point expirationTime)
        : orderType_{ orderType }
        , orderId_{ orderId }
        , side_{ side }
        , price_{ price }
        , initialQuantity_{ quantity }
        , remainingQuantity_{ quantity }
        , expirationTime_{ expirationTime }
    { }

    Order(OrderId orderId, Side side, Quantity quantity)
        : Order(OrderType::Market, orderId, side, Constants::InvalidPrice, quantity)
    { }

    OrderId GetOrderId() const { return orderId_; }
    Side GetSide() const { return side_; }
    Price GetPrice() const { return price_; }
    OrderType GetOrderType() const { return orderType_; }
    Quantity GetInitialQuantity() const { return initialQuantity_; }
    Quantity GetRemainingQuantity() const { return remainingQuantity_; }
    Quantity GetFilledQuantity() const { return GetInitialQuantity() - GetRemainingQuantity(); }
    bool IsFilled() const { return GetRemainingQuantity() == 0; }
    std::optional<std::chrono::system_clock::time_point> GetExpirationTime() const { return expirationTime_; }
    bool IsExpired() const 
    { 
        if (!expirationTime_.has_value())
            return false;
        return std::chrono::system_clock::now() >= expirationTime_.value();
    }
    void Fill(Quantity quantity)
    {
        if (quantity > GetRemainingQuantity())
            throw std::logic_error(std::format("Order ({}) cannot be filled for more than its remaining quantity.", GetOrderId()));

        remainingQuantity_ -= quantity;
    }
    void ToGoodTillCancel(Price price) 
    { 
        if (GetOrderType() != OrderType::Market)
            throw std::logic_error(std::format("Order ({}) cannot have its price adjusted, only market orders can.", GetOrderId()));

        price_ = price;
        orderType_ = OrderType::GoodTillCancel;
    }

private:
    OrderType orderType_;
    OrderId orderId_;
    Side side_;
    Price price_;
    Quantity initialQuantity_;
    Quantity remainingQuantity_;
    std::optional<std::chrono::system_clock::time_point> expirationTime_;
};

using OrderPointer = std::shared_ptr<Order>;
using OrderPointers = std::list<OrderPointer>;

