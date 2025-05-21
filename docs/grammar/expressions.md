$$
\begin{align*}
[\text{expression}] &\to [\text{term}]~[\text{expression'}]
\\
[\text{expression'}] &\to + [\text{term}]~[\text{expression'}] \mid - [\text{term}]~[\text{expression'}] \mid \epsilon
\\\\
[\text{term}] &\to [\text{factor}]~[\text{term'}]
\\
[\text{term'}] &\to * [\text{factor}]~[\text{term'}] \mid / [\text{factor}]~[\text{term'}] \mid \epsilon
\\\\
[\text{factor}] &\to \text{number} \mid ([\text{expression}])
\end{align*}
$$