library(tidyverse)
library(scales)
library(magrittr)

load_data <- function(filepath, label) {
  readr::read_csv(
    filepath,
    col_types = list(col_factor(), col_double(), col_double())
  ) %>%
    dplyr::mutate(
      machine = label,
      throughput = (2 * len / rtt) / 10^6
    )
}

plot_rtt <- function(df) {
  df %>%
    dplyr::filter(len <= 100) %>%
    dplyr::select(c(len, rtt, machine)) %>%
    ggplot2::ggplot(aes(colour = machine)) +
    ggplot2::geom_boxplot(
      aes(x = as.factor(len), y = rtt * 10^6)
    ) +
    ylim(0, 50) +
    ggplot2::theme_classic() +
    ggplot2::xlab("comm size [B]") +
    ggplot2::ylab("RTT [micro secs]") +
    ggplot2::theme(
      axis.title = ggplot2::element_text(size = 15, face = "bold"),
      axis.text = ggplot2::element_text(size = 15, face = "bold"),
      legend.text = ggplot2::element_text(size = 15, face = "bold"),
      legend.title = ggplot2::element_text(size = 15, face = "bold"),
    )
}

plot_throughput <- function(df) {
  df %>%
    dplyr::select(c(len, throughput, machine)) %>%
    ggplot2::ggplot(aes(colour = machine)) +
    ggplot2::geom_boxplot(
      aes(x = as.factor(len), y = throughput)
    ) +
    ggplot2::theme_classic() +
    ggplot2::xlab("comm size [B]") +
    ggplot2::ylab("Throughput [MB/s]") +
    ggplot2::theme(
      axis.title = ggplot2::element_text(size = 15, face = "bold"),
      axis.text = ggplot2::element_text(size = 15, face = "bold"),
      legend.text = ggplot2::element_text(size = 15, face = "bold"),
      legend.title = ggplot2::element_text(size = 15, face = "bold"),
    )
}
