#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <cmath>
#include <iostream>

namespace py = pybind11;


void softmax_regression_epoch_cpp(const float *X, const unsigned char *y,
                                  float *theta, size_t m, size_t n, size_t k,
                                  float lr, size_t batch)
{   
    /**
     * A C++ version of the softmax regression epoch code.  This should run a
     * single epoch over the data defined by X and y (and sizes m,n,k), and
     * modify theta in place.  Your function will probably want to allocate
     * (and then delete) some helper arrays to store the logits and gradients.
     *
     * Args:
     *     X (const float *): pointer to X data, of size m*n, stored in row
     *          major (C) format
     *     y (const unsigned char *): pointer to y data, of size m
     *     theta (float *): pointer to theta data, of size n*k, stored in row
     *          major (C) format
     *     m (size_t): number of examples
     *     n (size_t): input dimension
     *     k (size_t): number of classes
     *     lr (float): learning rate / SGD step size
     *     batch (int): SGD minibatch size
     *
     * Returns:
     *     (None)
     */

    /// BEGIN YOUR CODE
    // Allocate memory for the logits and gradients
    float *logits = new float[k];
    float *gradients = new float[n * k];  // Gradients for theta

    // Loop over the data in batches
    for (size_t i = 0; i < m; i += batch)
    {
        // Determine the size of the current batch
        size_t current_batch_size = std::min(batch, m - i);

        // Zero out the gradients
        for (size_t j = 0; j < n * k; j++)
        {
            gradients[j] = 0;  // Initialize gradients to zero
        }

        // Loop over the batch
        for (size_t j = 0; j < current_batch_size; j++)
        {
            // Compute the logits
            for (size_t l = 0; l < k; l++)
            {
                logits[l] = 0;
                for (size_t m = 0; m < n; m++)
                {
                    logits[l] += X[(i + j) * n + m] * theta[m * k + l];
                }
            }

            // Compute the softmax with numerical stability
            float max_logit = logits[0];
            for (size_t l = 1; l < k; l++)
            {
                if (logits[l] > max_logit)
                {
                    max_logit = logits[l];
                }
            }

            float sum = 0;
            for (size_t l = 0; l < k; l++)
            {
                logits[l] = exp(logits[l] - max_logit);
                sum += logits[l];
            }

            // Avoid division by zero
            for (size_t l = 0; l < k; l++)
            {
                logits[l] /= sum;
            }

            // Compute the gradients
            for (size_t l = 0; l < k; l++)
            {
                float label = (l == y[i + j]) ? 1.0f : 0.0f;
                for (size_t m = 0; m < n; m++)
                {
                    gradients[m * k + l] += (logits[l] - label) * X[(i + j) * n + m];
                }
            }
        }

        // Update theta after computing all gradients in the batch
        for (size_t l = 0; l < k; l++)
        {
            for (size_t m = 0; m < n; m++)
            {
                theta[m * k + l] -= lr * gradients[m * k + l] / current_batch_size;
            }
        }
    }

    // Free allocated memory
    delete[] logits;
    delete[] gradients;
    /// END YOUR CODE
}


/**
 * This is the pybind11 code that wraps the function above.  It's only role is
 * wrap the function above in a Python module, and you do not need to make any
 * edits to the code
 */
PYBIND11_MODULE(simple_ml_ext, m) {
    m.def("softmax_regression_epoch_cpp",
    	[](py::array_t<float, py::array::c_style> X,
           py::array_t<unsigned char, py::array::c_style> y,
           py::array_t<float, py::array::c_style> theta,
           float lr,
           int batch) {
        softmax_regression_epoch_cpp(
        	static_cast<const float*>(X.request().ptr),
            static_cast<const unsigned char*>(y.request().ptr),
            static_cast<float*>(theta.request().ptr),
            X.request().shape[0],
            X.request().shape[1],
            theta.request().shape[1],
            lr,
            batch
           );
    },
    py::arg("X"), py::arg("y"), py::arg("theta"),
    py::arg("lr"), py::arg("batch"));
}
