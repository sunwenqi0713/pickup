#pragma once

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

/*!
 * @file angles.h
 * @brief 定义了一些角度相关的工具函数
 * 角度与弧度的转换转换关系如下
 * radians = degrees x π / 180
 * degrees = radians x 180 / π
 */
namespace pickup {
namespace math {

/*!
 * @brief 将角度从度数转换为弧度
 * @param degrees 输入的角度值（度数）
 * @return 对应的弧度值
 */
double fromDegrees(double degrees);

/*!
 * @brief 将弧度转换为角度（度数）
 * @param radians 输入的弧度值
 * @return 对应的角度值（度数）
 */
double toDegrees(double radians);

/*!
 * @brief 将角度归一化到[0, 2π)区间
 * @param angle 输入的角度值（弧度）
 * @return 归一化后的角度值（弧度），范围[0, 2π)
 *
 * @details 该函数通过取模运算将输入角度映射到0到2π范围内，
 *          例如：3π 将被归一化为π
 */
double normalizeAnglePositive(double angle);

/*!
 * @brief 将角度归一化到[-π, π)区间
 * @param angle 输入的角度值（弧度）
 * @return 归一化后的角度值（弧度），范围[-π, π)
 *
 * @details 该函数先将角度归一化到[0, 2π)范围，再通过相位平移
 *          将其转换到[-π, π)范围。例如：3π/2 将被转换为-π/2
 */
double normalizeAngle(double angle);

}  // namespace math
}  // namespace pickup