<script lang="ts" setup>
import { NConfigProvider, NGlobalStyle, NMessageProvider } from "naive-ui";
</script>

<template>
  <NConfigProvider inline-theme-disabled preflight-style-disabled>
    <NMessageProvider closable>
      <NuxtLayout>
        <NuxtPage />
      </NuxtLayout>
    </NMessageProvider>
    <NGlobalStyle />
  </NConfigProvider>
</template>
