#ifndef LSTMACCELERATE_H_INCLUDED
#define LSTMACCELERATE_H_INCLUDED

#include "../common.h"

namespace RTNeural
{

template <typename T>
class LSTMLayer : public Layer<T>
{
public:
    LSTMLayer(size_t in_size, size_t out_size);
    virtual ~LSTMLayer();

    void reset() override;
    virtual inline void forward(const T* input, T* h) override
    {
        forward_internal(input, h);
    }

    void setWVals(const std::vector<std::vector<T>>& wVals);
    void setUVals(const std::vector<std::vector<T>>& uVals);
    void setBVals(const std::vector<T>& bVals);

protected:
    template <typename FloatType = T>
    inline typename std::enable_if<std::is_same<FloatType, float>::value>::type
    forward_internal(const float* input, float* h)
    {
        float dotpr_out;
        for(size_t i = 0; i < Layer<T>::out_size; ++i)
        {
            vDSP_dotpr(fWeights.W[i], 1, input, 1, &dotpr_out, Layer<T>::in_size);
            fVec[i] = dotpr_out;
            vDSP_dotpr(fWeights.U[i], 1, ht1, 1, &dotpr_out, Layer<T>::out_size);
            fVec[i] += dotpr_out;

            vDSP_dotpr(iWeights.W[i], 1, input, 1, &dotpr_out, Layer<T>::in_size);
            iVec[i] = dotpr_out;
            vDSP_dotpr(iWeights.U[i], 1, ht1, 1, &dotpr_out, Layer<T>::out_size);
            iVec[i] += dotpr_out;

            vDSP_dotpr(oWeights.W[i], 1, input, 1, &dotpr_out, Layer<T>::in_size);
            oVec[i] = dotpr_out;
            vDSP_dotpr(oWeights.U[i], 1, ht1, 1, &dotpr_out, Layer<T>::out_size);
            oVec[i] += dotpr_out;

            vDSP_dotpr(cWeights.W[i], 1, input, 1, &dotpr_out, Layer<T>::in_size);
            ctVec[i] = dotpr_out;
            vDSP_dotpr(cWeights.U[i], 1, ht1, 1, &dotpr_out, Layer<T>::out_size);
            ctVec[i] += dotpr_out;
        }

        vDSP_vadd(fVec, 1, fWeights.b, 1, fVec, 1, Layer<T>::out_size);
        sigmoid(fVec, fVec, Layer<T>::out_size);

        vDSP_vadd(iVec, 1, iWeights.b, 1, iVec, 1, Layer<T>::out_size);
        sigmoid(iVec, iVec, Layer<T>::out_size);

        vDSP_vadd(oVec, 1, oWeights.b, 1, oVec, 1, Layer<T>::out_size);
        sigmoid(oVec, oVec, Layer<T>::out_size);

        vDSP_vadd(ctVec, 1, cWeights.b, 1, ctVec, 1, Layer<T>::out_size);
        const auto dim_int = static_cast<int>(Layer<T>::out_size);
        vvtanhf(ctVec, ctVec, &dim_int);

        vDSP_vmul(fVec, 1, ct1, 1, cVec, 1, Layer<T>::out_size);
        vDSP_vmul(iVec, 1, ctVec, 1, ht1, 1, Layer<T>::out_size);
        vDSP_vadd(cVec, 1, ht1, 1, cVec, 1, Layer<T>::out_size);

        vvtanhf(h, cVec, &dim_int);
        vDSP_vmul(h, 1, oVec, 1, h, 1, Layer<T>::out_size);

        cblas_scopy(Layer<T>::out_size, cVec, 1, ct1, 1);
        cblas_scopy(Layer<T>::out_size, h, 1, ht1, 1);
    }

    template <typename FloatType = T>
    inline typename std::enable_if<std::is_same<FloatType, double>::value>::type
    forward_internal(const double* input, double* h)
    {
        double dotpr_out;
        for(size_t i = 0; i < Layer<T>::out_size; ++i)
        {
            vDSP_dotprD(fWeights.W[i], 1, input, 1, &dotpr_out, Layer<T>::in_size);
            fVec[i] = dotpr_out;
            vDSP_dotprD(fWeights.U[i], 1, ht1, 1, &dotpr_out, Layer<T>::out_size);
            fVec[i] += dotpr_out;

            vDSP_dotprD(iWeights.W[i], 1, input, 1, &dotpr_out, Layer<T>::in_size);
            iVec[i] = dotpr_out;
            vDSP_dotprD(iWeights.U[i], 1, ht1, 1, &dotpr_out, Layer<T>::out_size);
            iVec[i] += dotpr_out;

            vDSP_dotprD(oWeights.W[i], 1, input, 1, &dotpr_out, Layer<T>::in_size);
            oVec[i] = dotpr_out;
            vDSP_dotprD(oWeights.U[i], 1, ht1, 1, &dotpr_out, Layer<T>::out_size);
            oVec[i] += dotpr_out;

            vDSP_dotprD(cWeights.W[i], 1, input, 1, &dotpr_out, Layer<T>::in_size);
            ctVec[i] = dotpr_out;
            vDSP_dotprD(cWeights.U[i], 1, ht1, 1, &dotpr_out, Layer<T>::out_size);
            ctVec[i] += dotpr_out;
        }

        vDSP_vaddD(fVec, 1, fWeights.b, 1, fVec, 1, Layer<T>::out_size);
        sigmoid(fVec, fVec, Layer<T>::out_size);

        vDSP_vaddD(iVec, 1, iWeights.b, 1, iVec, 1, Layer<T>::out_size);
        sigmoid(iVec, iVec, Layer<T>::out_size);

        vDSP_vaddD(oVec, 1, oWeights.b, 1, oVec, 1, Layer<T>::out_size);
        sigmoid(oVec, oVec, Layer<T>::out_size);

        vDSP_vaddD(ctVec, 1, cWeights.b, 1, ctVec, 1, Layer<T>::out_size);
        const auto dim_int = static_cast<int>(Layer<T>::out_size);
        vvtanh(ctVec, ctVec, &dim_int);

        vDSP_vmulD(fVec, 1, ct1, 1, cVec, 1, Layer<T>::out_size);
        vDSP_vmulD(iVec, 1, ctVec, 1, ht1, 1, Layer<T>::out_size);
        vDSP_vaddD(cVec, 1, ht1, 1, cVec, 1, Layer<T>::out_size);

        vvtanh(h, cVec, &dim_int);
        vDSP_vmulD(h, 1, oVec, 1, h, 1, Layer<T>::out_size);

        cblas_dcopy((int) Layer<T>::out_size, cVec, 1, ct1, 1);
        cblas_dcopy((int) Layer<T>::out_size, h, 1, ht1, 1);
    }

    T* ht1;
    T* ct1;

    struct WeightSet
    {
        WeightSet(size_t in_size, size_t out_size);
        ~WeightSet();

        T** W;
        T** U;
        T* b;
        const size_t out_size;
    };

    WeightSet fWeights;
    WeightSet iWeights;
    WeightSet oWeights;
    WeightSet cWeights;

    T* fVec;
    T* iVec;
    T* oVec;
    T* ctVec;
    T* cVec;
};

} // namespace RTNeural

#endif // LSTMACCELERATE_H_INCLUDED
