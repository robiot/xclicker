import React, { useEffect } from "react";
import Head from "next/head";

export function Layout({ children, title, home = false }) {
  return (
    <div className="min-h-screen bg-gray-900">
      <Head>
        <title>{home ? `XClicker - ${title}` : `${title} - XClicker`}</title>
        <meta charSet="utf-8" />
        <meta name="viewport" content="initial-scale=1.0, width=device-width" />
        <meta name="Description" content="A fast gui autoclicker for linux" />
        <meta content="https://xclicker.pages.dev" property="og:url" />
        <meta content="/embed.png" property="og:image" />
        <meta content="#43B581" data-react-helmet="true" name="theme-color" />
        <link rel="icon" href="/favicon.ico" />
      </Head>

      {children}
    </div>
  );
}

export default Layout;
